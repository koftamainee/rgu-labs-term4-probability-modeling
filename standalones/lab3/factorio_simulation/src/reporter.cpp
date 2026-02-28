#include "reporter.hpp"
#include "recipe.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <iomanip>

Reporter::Reporter(const std::filesystem::path &output_path, bool compile_pdf)
    : m_tex_path(output_path.string() + ".tex")
    , m_pdf_path(output_path.string() + ".pdf")
    , m_compile(compile_pdf)
{}

void Reporter::write(const ReportConfig &config, const SimulationResult& result) const {
    {
        const std::string doc = build_document(config, result);
        std::ofstream f(m_tex_path);
        if (!f) throw std::runtime_error("Cannot open " + m_tex_path.string());
        f << doc;
    }

    if (m_compile) {
        const std::filesystem::path abs = std::filesystem::absolute(m_tex_path);
        const std::string tex_dir  = abs.parent_path().string();
        const std::string tex_file = abs.filename().string();
        const std::string cmd =
            "cd \"" + tex_dir + "\" && "
            "xelatex -interaction=nonstopmode \"" + tex_file + "\" > /dev/null 2>&1 && "
            "xelatex -interaction=nonstopmode \"" + tex_file + "\" > /dev/null 2>&1";
        const int ret = std::system(cmd.c_str());
        if (ret != 0) {
            std::cerr << "[Reporter] xelatex failed (code " << ret
                      << "). .tex file saved to " << m_tex_path << "\n";
        } else {
            std::cerr << "[Reporter] PDF written to " << m_pdf_path << "\n";
        }
    } else {
        std::cerr << "[Reporter] .tex written to " << m_tex_path << "\n";
    }
}

std::string Reporter::escape(const std::string& s) {
    std::string out;
    out.reserve(s.size());
    for (const char c : s) {
        switch (c) {
            case '&': out += "\\&"; break;
            case '%': out += "\\%"; break;
            case '$': out += "\\$"; break;
            case '#': out += "\\#"; break;
            case '_': out += "\\_"; break;
            case '{': out += "\\{"; break;
            case '}': out += "\\}"; break;
            case '~': out += "\\textasciitilde{}"; break;
            case '^': out += "\\textasciicircum{}"; break;
            case '\\': out += "\\textbackslash{}"; break;
            default:  out += c; break;
        }
    }
    return out;
}

std::string Reporter::build_pgfplot_coords(const std::vector<int>& ticks) {
    std::ostringstream ss;
    for (int i = 0; i < static_cast<int>(ticks.size()); i++) {
        ss << "(" << (i + 1) << "," << ticks[i] << ")";
    }
    return ss.str();
}

std::string Reporter::format_ticks_table(const std::vector<int>& ticks, int cols) {
    if (ticks.empty()) return "---\n";

    std::ostringstream ss;
    ss << "\\begin{center}\n\\begin{longtable}{";
    for (int c = 0; c < cols; c++) ss << "r";
    ss << "}\n\\hline\n";

    for (int i = 0; i < static_cast<int>(ticks.size()); i++) {
        if (i > 0 && i % cols == 0) ss << "\\\\\n";
        else if (i % cols != 0)      ss << " & ";
        ss << ticks[i];
    }
    const int remainder = static_cast<int>(ticks.size()) % cols;
    if (remainder != 0) {
        for (int p = remainder; p < cols; p++) ss << " & ";
    }
    ss << "\\\\\n\\hline\n\\end{longtable}\n\\end{center}\n";
    return ss.str();
}

std::string Reporter::build_document(const ReportConfig& config,
                                     const SimulationResult& result) {
    const int finish_tick = result.legendary_ticks.empty()
                          ? 0 : result.legendary_ticks.back();

    auto inter_arrivals = [](const std::vector<int>& ticks) -> std::vector<int> {
        std::vector<int> d;
        for (int i = 1; i < static_cast<int>(ticks.size()); i++)
            d.push_back(ticks[i] - ticks[i-1]);
        return d;
    };

    auto mean_val = [](const std::vector<int>& v) -> double {
        if (v.empty()) return 0.0;
        double s = 0; for (const int x : v) s += x; return s / v.size();
    };

    const auto leg_ia = inter_arrivals(result.legendary_ticks);
    const double leg_mean = mean_val(leg_ia);

    std::ostringstream tex;

    tex << R"(\documentclass[12pt,a4paper]{article}
\usepackage{geometry}
\geometry{margin=2cm}
\usepackage{booktabs}
\usepackage{longtable}
\usepackage{pgfplots}
\pgfplotsset{compat=1.18}
\usepackage{pgfplotstable}
\usepackage{xcolor}
\usepackage{hyperref}
\usepackage{caption}
\usepackage{amsmath}

\definecolor{clrRare}{HTML}{0055AA}
\definecolor{clrEpic}{HTML}{7700BB}
\definecolor{clrLeg}{HTML}{CC6600}

\title{\textbf{Simulation Report: Nauvis Factory}}
\date{\today}
\author{Factorio Simulation}

\begin{document}
\maketitle
\tableofcontents
\newpage
)";

    tex << R"(\section{Simulation Parameters}

\begin{tabular}{ll}
\toprule
\textbf{Parameter} & \textbf{Value} \\
\midrule
)";
    tex << "Assemblers ($AM$) & " << config.assembler_count << " \\\\\n";
    tex << "Recyclers ($RM$) & " << config.recycler_count << " \\\\\n";
    tex << "Quality modules in assembler & " << config.assembler_quality_modules << " \\\\\n";
    tex << "Quality modules in recycler & " << config.recycler_quality_modules << " \\\\\n";
    tex << "Craft time ($t_{\\text{craft}}$) & " << config.base_craft_time << " ticks \\\\\n";
    tex << "Recycle time ($t_{\\text{rec}}$) & " << config.base_recycle_time << " ticks \\\\\n";
    tex << "Goal & " << config.legendary_goal << " legendary \\\\\n";
    tex << "Finished at tick & " << finish_tick << " \\\\\n";
    tex << R"(\bottomrule
\end{tabular}

\subsection*{Recipe}
\begin{tabular}{lll}
\toprule
\textbf{Component} & \textbf{Count} & \textbf{Supply rate} \\
\midrule
)";
    for (int i = 0; i < static_cast<int>(config.ingredients.size()); i++) {
        const int rate = (i < static_cast<int>(config.supply_rates.size()))
                       ? config.supply_rates[i] : 0;
        tex << escape(config.ingredients[i].name) << " & "
            << config.ingredients[i].count << " & "
            << rate << " /tick \\\\\n";
    }
    tex << R"(\bottomrule
\end{tabular}

)";

    tex << R"(\section{Simulation Results}

\begin{tabular}{llll}
\toprule
\textbf{Quality level} & \textbf{Name} & \textbf{Count} & \textbf{Last tick} \\
\midrule
)";
    auto last_or = [](const std::vector<int>& v) -> std::string {
        return v.empty() ? "---" : std::to_string(v.back());
    };
    tex << "3 & Rare & " << result.rare_ticks.size()
        << " & " << last_or(result.rare_ticks) << " \\\\\n";
    tex << "4 & Epic & " << result.epic_ticks.size()
        << " & " << last_or(result.epic_ticks) << " \\\\\n";
    tex << "5 & Legendary & " << result.legendary_ticks.size()
        << " & " << last_or(result.legendary_ticks) << " \\\\\n";
    tex << R"(\bottomrule
\end{tabular}

)";

    tex << R"(\section{Graphs}

\begin{figure}[h!]
\centering
\begin{tikzpicture}
\begin{axis}[
    width=14cm, height=7cm,
    xlabel={Tick},
    ylabel={Cumulative count},
    legend pos=north west,
    grid=major,
    grid style={dashed,gray!40},
]
)";

    if (!result.rare_ticks.empty()) {
        tex << "\\addplot[color=clrRare, thick] coordinates {";
        for (int i = 0; i < static_cast<int>(result.rare_ticks.size()); i++)
            tex << "(" << result.rare_ticks[i] << "," << (i+1) << ")";
        tex << "};\n\\addlegendentry{Rare}\n";
    }
    if (!result.epic_ticks.empty()) {
        tex << "\\addplot[color=clrEpic, thick, dashed] coordinates {";
        for (int i = 0; i < static_cast<int>(result.epic_ticks.size()); i++)
            tex << "(" << result.epic_ticks[i] << "," << (i+1) << ")";
        tex << "};\n\\addlegendentry{Epic}\n";
    }
    if (!result.legendary_ticks.empty()) {
        tex << "\\addplot[color=clrLeg, very thick] coordinates {";
        for (int i = 0; i < static_cast<int>(result.legendary_ticks.size()); i++)
            tex << "(" << result.legendary_ticks[i] << "," << (i+1) << ")";
        tex << "};\n\\addlegendentry{Legendary}\n";
    }

    tex << R"(\end{axis}
\end{tikzpicture}
\caption{Cumulative number of produced items per quality level}
\end{figure}

)";

    if (leg_ia.size() >= 2) {
        tex << R"(
\begin{figure}[h!]
\centering
\begin{tikzpicture}
\begin{axis}[
    width=14cm, height=6cm,
    xlabel={Legendary item index},
    ylabel={Interval (ticks)},
    ybar, bar width=6pt,
    xtick=data,
    grid=major,
    grid style={dashed,gray!40},
    color=clrLeg,
]
\addplot[fill=clrLeg!60, draw=clrLeg] coordinates {
)";
        for (int i = 0; i < static_cast<int>(leg_ia.size()); i++)
            tex << "(" << (i+2) << "," << leg_ia[i] << ")";
        tex << R"(};
\end{axis}
\end{tikzpicture}
\caption{Intervals between consecutive legendary items. Mean: )";
        tex << std::fixed << std::setprecision(1) << leg_mean;
        tex << R"( ticks.}
\end{figure}

)";
    }

    tex << R"(\section{Production Tick Collections}

\subsection{Rare [level 3] --- )" << result.rare_ticks.size() << " items}\n\n";
    tex << format_ticks_table(result.rare_ticks);

    tex << R"(\subsection{Epic [level 4] --- )" << result.epic_ticks.size() << " items}\n\n";
    tex << format_ticks_table(result.epic_ticks);

    tex << R"(\subsection{Legendary [level 5] --- )" << result.legendary_ticks.size() << " items}\n\n";
    tex << format_ticks_table(result.legendary_ticks, 5);

    tex << "\n\\end{document}\n";
    return tex.str();
}