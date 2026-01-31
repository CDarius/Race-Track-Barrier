<script setup lang="ts">

</script>

<template>
    <div class="main-container">
        <div class="header-content">
            <slot name="header"></slot>
        </div>
        <div class="scrollable-content">
            <slot></slot>
        </div>
    </div>
</template>

<style scoped>
.main-container {
    display: flex;
    flex-direction: column;
    height: 100%;
    /* Full viewport height */
}

.header-content {
    flex-shrink: 0 !important;
    /* Prevent shrinking */
}

.scrollable-content {
    flex-grow: 1 !important;
    /* Takes up the remaining space */
    overflow-y: auto !important;
    /* Enables vertical scrolling */
}
</style>
