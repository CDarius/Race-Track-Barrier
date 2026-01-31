export interface GameFunctionItem {
    name: string,
    title: string,
    description: string,
    prerequisites: string[]
};

export interface GameFunctionsGroup {
    name: string,
    title: string,
    functions: GameFunctionItem[]
}
    
export interface ListGameFunctionsResponse {
    functions: GameFunctionsGroup[];
}

export enum FunctionStatus {
    Done = 'Done',
    InProgress = 'InProgress',
    Failed = 'Failed',
    Unknown = 'Unknown'
}

export interface GetFunctionStatusResponse {
    group: string;
    functionName: string;
    action: string;
    status: FunctionStatus;
    failure_description: string | null;
}

export interface GetFunctionPrerequisitesResponse {
    group: string;
    functionName: string;
    action: string;
    prerequisites_met: boolean[];
}

export interface GetFunctionStartResponse {
    group: string;
    functionName: string;
    action: string;
    status: FunctionStatus;
    failure_description: string | null;
}

