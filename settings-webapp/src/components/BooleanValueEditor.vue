<script setup lang="ts">

const props = withDefaults(defineProps<{
    value: boolean,
    updateInProgress?: boolean,
}>(), {
    updateInProgress: false,
});

const emit = defineEmits<{
    (e: 'updateValue', value: boolean): void;
}>();


function updateValue(event: Event) {
    console.log('Checkbox change event:', event);
    let newValue = (event.target as HTMLInputElement).checked
    if (props.value != newValue) {
        emit('updateValue', newValue);
    }

}
</script>

<template>
    <div class="switch-container position-relative d-inline-block">
        <div class="form-check form-switch">
            <input class="form-check-input" type="checkbox" id="flexSwitchCheckDefault"
                :checked="props.value" @change="updateValue($event)" 
                :disabled="props.updateInProgress" style="caret-color: transparent;"/>
        </div>
        <div v-if="props.updateInProgress" class="spinner-overlay">
            <div class="spinner-border text-primary" role="status" style="width: 1.5rem; height: 1.5rem;">
                <span class="visually-hidden">Loading...</span>
            </div>
        </div>
    </div>
</template>

<style scoped>

.switch-container {
    /* Ensures the overlay is positioned relative to the switch */
    position: relative;
    display: inline-block;
}

.spinner-overlay {
    position: absolute;
    top: 50%;
    left: 50%;
    transform: translate(-50%, -50%);
    pointer-events: none; /* Allows clicks to pass through to the switch */
    z-index: 2;
}

</style>