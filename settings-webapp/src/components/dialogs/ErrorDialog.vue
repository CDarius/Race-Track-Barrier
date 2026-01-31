<template>
    <GenericDialog :title="title" titleClass="text-bg-danger" title-icon="bi-exclamation-triangle-fill" :buttons="[
        { text: buttonText ?? 'Chiudi', class: 'btn-secondary' },
    ]" :isOpen="isOpen" @update:isOpen="closeDialog">
        {{ message }}
    </GenericDialog>
</template>

<script setup lang="ts">
import GenericDialog from './GenericDialog.vue';

defineProps<{
    title: string;
    message: string;
    buttonText?: string;
    isOpen: boolean;
}>();

const emit = defineEmits(['update:isOpen']);

function closeDialog() {
    emit('update:isOpen', false);
}

</script>