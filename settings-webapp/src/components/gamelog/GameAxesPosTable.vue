<template>
    <table class="table table-bordered table-sm text-center">
        <thead>
            <tr class="table-secondary">
                <th scope="col">Time (ms)</th>
                <th scope="col">Ball X</th>
                <th scope="col">Ball Y</th>
                <th scope="col">Paddle L</th>
                <th scope="col">Paddle R</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td>{{ time }}</td>
                <td>{{ ballX }}</td>
                <td>{{ ballY }}</td>
                <td>{{ paddleL }}</td>
                <td>{{ paddleR }}</td>
            </tr>
        </tbody>
    </table>
</template>

<script setup lang="ts">

var props = defineProps<{
    time: number,
    ballX: number,
    ballY: number,
    paddleL: number,
    paddleR: number,
}>();

</script>