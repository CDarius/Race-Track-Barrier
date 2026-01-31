<template>
    <div class="spinner-backdrop d-flex flex-column justify-content-center align-items-center">
        <h3>{{ props.text }}</h3>
        <div class="spinner-border mt-2" style="width: 3rem; height: 3rem;" role="status">
            <span class="visually-hidden">{{ props.text }}</span>
        </div>
    </div>
</template>

<script setup lang="ts">

const props = defineProps<{
    text: string,
}>();

</script>

<style scoped>
.spinner-backdrop {
    position: fixed;
    top: 0;
    left: 0;
    width: 100vw;
    height: 100vh;
    background: rgba(0, 0, 0, 0.25);
    z-index: 100;
    display: flex;
    align-items: center;
    justify-content: center;
}
</style>