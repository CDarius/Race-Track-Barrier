export interface GameSettingItem {
    name: string;
    title: string;
    description: string;
    unit: string;
    type: string;
    minValue?: number;
    maxValue?: number;
    stepChange?: number;
}

export interface GameSettingsGroup {
    name: string;
    title: string;
    settings: GameSettingItem[];
}

export interface ListGameSettingsResponse {
    groups: GameSettingsGroup[];
}

export interface ReadGameSettingRespose {
    group: string;
    name: string;
    value: number|boolean;
}

export interface WriteGameSettingRespose {
    group: string;
    name: string;
    value: number|boolean;
}