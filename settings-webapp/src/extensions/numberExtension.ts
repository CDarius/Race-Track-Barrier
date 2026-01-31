export {};

declare global {
    interface Number {
      roundDecimalToDigits(digits: number): number;
      getNumOfDecimalDigits(): number;
    }
}
  
Number.prototype.roundDecimalToDigits = function (this: number, digits: number): number {
    const factor = Math.pow(10, digits);
    return Math.round(this * factor) / factor;
};
  
Number.prototype.getNumOfDecimalDigits = function (this: number): number {
    const decimalPart = this.toString().split('.')[1];
    return decimalPart ? decimalPart.length : 0;
};
  