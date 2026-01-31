// Deep compare two object and return true if they are equal
export function deepEqual(a: any, b: any): boolean {
    return JSON.stringify(a) === JSON.stringify(b);
}
