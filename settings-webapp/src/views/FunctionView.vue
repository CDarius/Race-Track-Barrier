<template>
    <PageScrollableLayout v-if="webFunction">
        <div class="py-2 ps-2 bg-light">
            <h1>{{ webFunction.title }}</h1>
            <p>{{ webFunction.description }}</p>
        </div>

        <ExecuteWebFunction :function-group-name="functionsGroup!.name"
            :web-function-name="webFunction!.name" 
            class="mx-2 mt-2"/>
    </PageScrollableLayout>
</template>

<script setup lang="ts">
import { computed } from 'vue';
import { useRoute } from 'vue-router';
import { useGameFunctionsStore } from '@/stores/gameFunctions'
import PageScrollableLayout from '@/components/PageScrollableLayout.vue';
import ExecuteWebFunction from '@/components/functions/ExecuteWebFunction.vue';

const route = useRoute();
const functionsStore = useGameFunctionsStore();
const functionsGroup = computed(() => functionsStore.groups.find((x) => x.name == route.params.group));
const webFunction = computed(() => functionsGroup.value?.functions.find((x) => x.name == route.params.function));

</script>
