import { Pipe, PipeTransform } from '@angular/core';

@Pipe({
  name: 'localeNumber',
  standalone: true
})
export class LocaleNumberPipe implements PipeTransform {
  transform(value: number | null | undefined, minimumFractionDigits = 0, maximumFractionDigits = 0): string {
    if (value === null || value === undefined) {
      return '';
    }

    const locale = navigator.language || 'en-US';
    
    return new Intl.NumberFormat(locale, {
      minimumFractionDigits,
      maximumFractionDigits
    }).format(value);
  }
}

