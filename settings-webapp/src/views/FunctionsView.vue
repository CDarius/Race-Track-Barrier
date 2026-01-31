<template>
    <div v-if="functionsStore.groupsLoading"
        class="h-100 w-100 d-flex flex-column justify-content-center align-items-center">
        <h3>Loading...</h3>
        <div class="spinner-border mt-2" style="width: 3rem; height: 3rem;" role="status">
            <span class="visually-hidden">Loading...</span>
        </div>
    </div>

    <div v-else-if="functionsStore.groupsLoadingError" class="container-md">
        <div class="alert alert-danger d-flex align-items-center mt-4" role="alert">
            <i class="bi bi-exclamation-triangle-fill me-2"></i>
            <div>
                {{ functionsStore.groupsLoadingError }}
            </div>
        </div>
    </div>

    <PageScrollableLayout v-else>
        <template v-slot:header>
            <SaveReloadSettings />
        </template>

        <div class="container-md my-3">
            <div class="list-group">
                <router-link v-for="group in functionsStore.groups" :to="'/functions/' + group.name" :key="group.name"
                    class="list-group-item list-group-item-action">
                    {{ group.title }}
                    <i class="bi bi-chevron-right float-end"></i>
                </router-link>
            </div>
        </div>
    </PageScrollableLayout>
</template>

<script setup lang="ts">
import { onBeforeMount } from 'vue';
import { useGameFunctionsStore } from '@/stores/gameFunctions';
import PageScrollableLayout from '@/components/PageScrollableLayout.vue';
import SaveReloadSettings from '@/components/settings/SaveReloadSettings.vue';

const functionsStore = useGameFunctionsStore();

onBeforeMount(() => {
    if (functionsStore.groups == null || functionsStore.groups.length == 0)
        functionsStore.fetchGameFunctionsList();
});

</script>

<style scoped></style>