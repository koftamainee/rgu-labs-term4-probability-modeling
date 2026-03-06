<template>
  <div id="app">
    <nav class="navbar">
      <span class="brand">Simulation App</span>
      <router-link to="/">Home</router-link>
    </nav>
    <main class="main-content">
      <router-view />
    </main>
  </div>
</template>

<script setup>
</script>

<style>
.navbar {
  display: flex;
  align-items: center;
  gap: 1.5rem;
  padding: 0.75rem 2rem;
  background: var(--surface);
  border-bottom: 1px solid var(--border);
}
.navbar .brand { font-weight: 700; color: var(--accent); margin-right: auto; }
.navbar a      { color: var(--muted); text-decoration: none; font-size: .9rem; }
.navbar a:hover,
.navbar a.router-link-active { color: var(--txt); }

.main-content {
  max-width: 1200px;
  margin: 2rem auto;
  padding: 0 1rem;
}
</style>