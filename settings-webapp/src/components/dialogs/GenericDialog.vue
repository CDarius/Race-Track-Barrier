<template>
    <Transition>
        <div v-if="isOpen" class="dialog-backdrop" @click="closeDialogBackdrop">
            <dialog class="rounded border border-0 p-0" open>
                <div class="card border border-0">
                    <div v-if="title" class="card-header d-flex justify-content-between align-items-center"
                        :class="titleClass">
                        <span>
                            <i v-if="titleIcon" class="bi me-1" :class="titleIcon"></i>
                            {{ title }}
                        </span>
                        <a @click="closeDialog">
                            <i class="bi bi-x-lg"></i>
                        </a>
                    </div>
                    <div class="card-body">
                        <slot></slot>
                        <div class="text-end mt-4" v-if="buttons && buttons.length">
                            <button v-for="(btn, idx) in buttons" :key="idx" class="btn ms-2" :class="btn.class"
                                @click="buttonClicked(btn)">
                                {{ btn.text }}
                            </button>
                        </div>
                    </div>
                </div>
            </dialog>
        </div>
    </Transition>
</template>

<style scoped>
/* Opacity transition for backdrop */
.dialog-backdrop {
    position: fixed;
    top: 0;
    left: 0;
    width: 100vw;
    height: 100vh;
    background: rgba(0, 0, 0, 0.5);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.v-enter-active,
.v-leave-active {
    transition: opacity 0.5s ease;
}

.v-enter-from,
.v-leave-to {
    opacity: 0;
}

.v-enter-active dialog {
    transition: transform 0.3s ease-in;
}

.v-enter-active dialog,
.v-leave-active dialog {
    transition: transform 0.3s ease-out;
}

.v-enter-from dialog {
    transform: translateY(-40%) scale(80%);
}

.v-leave-to dialog {
    transform: translateY(40%) scale(80%);
}

dialog {
    max-width: 90%;
}
</style>

<script setup lang="ts">

type DialogButton = {
    text: string;
    class?: string;
    callback?: () => void;
};

const props = defineProps<{
    title?: string;
    titleClass?: string;
    titleIcon?: string;
    isOpen: boolean;
    buttons?: DialogButton[];
}>();

const emit = defineEmits(['update:isOpen']);

function closeDialog() {
    emit('update:isOpen', false);
}

function closeDialogBackdrop() {
    if (!props.title) {
        closeDialog();
    }
}

function buttonClicked(button: DialogButton) {
    closeDialog();
    button.callback?.();
}

</script>