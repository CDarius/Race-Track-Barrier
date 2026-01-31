<template>
    <table class="table table-bordered table-sm text-center">
        <thead>
            <tr class="table-secondary">
                <th scope="col">Subcycle</th>
                <th scope="col">Ball Speed X</th>
                <th scope="col">Ball Speed Y</th>
                <th scope="col">Ball Target X</th>
                <th scope="col">Ball Target Y</th>
                <th scope="col">Paddle Target L</th>
                <th scope="col">Paddle Target R</th>
            </tr>
        </thead>
        <tbody>
            <tr v-for="subCycle in subCycles" :key="subCycle.subcycle">
                <td>{{ subCycle.subcycle }}</td>
                <td>{{ subCycle.speedX }}</td>
                <td>{{ subCycle.speedY }}</td>
                <td>{{ subCycle.targetBallX }}</td>
                <td>{{ subCycle.targetBallY }}</td>
                <td>{{ subCycle.targetPaddleL }}</td>
                <td>{{ subCycle.targetPaddleR }}</td>
            </tr>
        </tbody>
    </table>
</template>

<script setup lang="ts">

import { GameLogSubCycle } from '@/stores/gameLog';

var props = defineProps<{
    subCycles: GameLogSubCycle[],
}>();

</script>