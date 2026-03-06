import { createRouter, createWebHistory } from 'vue-router'
import HomeView from '@/views/HomeView.vue'

const routes = [
  {
    path: '/',
    name: 'home',
    component: HomeView
  }
  // Add more routes per task:
  // { path: '/task1', name: 'task1', component: () => import('@/views/Task1View.vue') }
]

export default createRouter({
  history: createWebHistory(),
  routes
})
