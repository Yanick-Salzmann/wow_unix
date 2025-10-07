import { ApplicationConfig, provideBrowserGlobalErrorListeners, provideZonelessChangeDetection, LOCALE_ID } from '@angular/core';
import { provideRouter } from '@angular/router';

import { routes } from './app.routes';

export const appConfig: ApplicationConfig = {
  providers: [
    provideBrowserGlobalErrorListeners(),
    provideZonelessChangeDetection(),
    provideRouter(routes),
    { provide: LOCALE_ID, useFactory: () => {
        const rawLocale = navigator.language || 'en-US';
        const locale = rawLocale.replace('_', '-');
        console.log('Raw navigator.language:', rawLocale);
        console.log('Using locale:', locale);
        console.log('All navigator.languages:', navigator.languages);
        return locale;
      }
    }
  ]
};
