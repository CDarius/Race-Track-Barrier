<script setup lang="ts">
import { computed } from 'vue';
import { useRoute } from 'vue-router';
import { RouterLink, RouterView } from 'vue-router'

const route = useRoute();
const appTitle = import.meta.env.VITE_APP_TITLE;
const breadcrumbs = computed(() => splitRoutePath(route.path));

function splitRoutePath(input: string): { name: string; path: string }[] {
    const parts = input.split('/').filter(Boolean); // removes empty strings
    const result: { name: string; path: string }[] = [];

    let currentPath = '';
    for (const part of parts) {
        currentPath += `/${part}`;
        result.push({
            name: part,
            path: currentPath,
        });
    }

    return result;
}

</script>

<template>
    <div class="top-header">
        <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
            <div class="container-fluid">
                <router-link class="navbar-brand" to="/">{{ appTitle }}</router-link>
                <button class="navbar-toggler" type="button" data-bs-toggle="collapse" data-bs-target="#navbarNav"
                    aria-controls="navbarNav" aria-expanded="false" aria-label="Toggle navigation">
                    <span class="navbar-toggler-icon"></span>
                </button>
                <div class="collapse navbar-collapse" id="navbarNav">
                    <ul class="navbar-nav">
                        <li class="nav-item">
                            <router-link class="nav-link" to="/settings">Settings</router-link>
                        </li>
                        <li class="nav-item">
                            <router-link class="nav-link" to="/functions">Functions</router-link>
                        </li>
                    </ul>
                </div>
            </div>
        </nav>
    </div>

    <div class="header bg-dark-subtle">
        <nav aria-label="breadcrumb">
            <ol class="breadcrumb m-0">
                <li class="breadcrumb-item">
                    <router-link to="/">Home</router-link>
                </li>
                <li v-for="(crumb, index) in breadcrumbs" :key="index" class="breadcrumb-item"
                    :class="{ active: index === breadcrumbs.length - 1 }" aria-current="page">
                    <router-link v-if="index !== breadcrumbs.length - 1" :to="crumb.path">
                        {{ crumb.name }}
                    </router-link>
                    <span v-else>{{ crumb.name }}</span>
                </li>
            </ol>
        </nav>

    </div>

    <div class="main-content">
        <RouterView />
    </div>
</template>

<style scoped>
.top-header {
    flex-shrink: 0;
    /* Prevent shrinking */
}

.main-content {
    flex-grow: 1;
    /* Takes up the remaining space */
    display: flex;
    flex-direction: column;
    overflow-y: auto;
    /* Takes up the remaining space */
}
</style>
