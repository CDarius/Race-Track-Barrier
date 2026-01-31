<script setup lang="ts">
import { computed, ref } from 'vue';
import { useRoute } from 'vue-router';
import { useGameSettingsStore } from '@/stores/gameSettings'
import PageScrollableLayout from '@/components/PageScrollableLayout.vue'
import EditSetting from '@/components/settings/EditSetting.vue';

const route = useRoute();
const settingStore = useGameSettingsStore();
const settingGroup = computed(() => settingStore.groups.find((x) => x.name == route.params.group));
const editSettingRefs = ref<any[]>([]);

function reloadSettingsValue() {
    // Call reload method on each EditSetting component
    editSettingRefs.value.forEach(refComp => {
        if (refComp && typeof refComp.fetchSettingValue === 'function') {
            refComp.fetchSettingValue();
        }
    });
}

</script>

<template>
    <PageScrollableLayout>
        <template v-slot:header>
            <div class="py-2 px-2 bg-light d-flex align-items-center justify-content-between">
                <h1>{{ settingGroup?.title ?? "???"}}</h1>
                <!-- Square Bootstrap button with icon -->
                <button type="button" class="btn btn-outline-dark rounded-square btn-sm"
                    v-on:click="reloadSettingsValue">
                    <i class="bi bi-arrow-clockwise"></i>
                </button>
            </div>
            
        </template>

        <div v-if="settingGroup != null" class="container-md my-3">
            <EditSetting
                v-for="(setting, idx) in settingGroup.settings"
                :key="setting.name"
                :group-name="settingGroup.name"
                :setting="setting"
                :class="'mb-3'"
                :ref="el => editSettingRefs[idx] = el"
            />
        </div>
    </PageScrollableLayout>
</template>