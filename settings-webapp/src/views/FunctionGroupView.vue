<script setup lang="ts">
import { computed } from 'vue';
import { useRoute } from 'vue-router';
import { useGameFunctionsStore } from '@/stores/gameFunctions'
import PageScrollableLayout from '@/components/PageScrollableLayout.vue'

const route = useRoute();
const functionsStore = useGameFunctionsStore();
const functionsGroup = computed(() => functionsStore.groups.find((x) => x.name == route.params.group));

</script>

<template>
    <PageScrollableLayout v-if="functionsGroup">
        <template v-slot:header>
            <div class="py-2 ps-2 bg-light">
                <h1>{{ functionsGroup?.title ?? "???" }}</h1>
            </div>
        </template>

        <!-- <div v-if="functionsGroup != null" class="container-md my-3">
            <HandleFunction v-for="webFunction in functionsGroup.functions" :key="webFunction.name"
                :group-name="functionsGroup.name" :web-function="webFunction" :class="'mb-3'">
            </HandleFunction>
        </div> -->

        <div class="container-md my-3">
            <div class="list-group">
                <router-link v-for="webFunction in functionsGroup.functions"
                    :to="'/functions/' + functionsGroup.name + '/' + webFunction.name" :key="webFunction.name"
                    class="list-group-item list-group-item-action">
                    {{ webFunction.title }}
                    <i class="bi bi-chevron-right float-end"></i>
                </router-link>
            </div>
        </div>

    </PageScrollableLayout>
</template>