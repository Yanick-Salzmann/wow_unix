import { Routes } from '@angular/router';
import { SplashComponent } from './splash/splash.component';

export const routes: Routes = [
  { path: 'splash', component: SplashComponent },
  { path: '', redirectTo: 'splash', pathMatch: 'full' },
  { path: '**', redirectTo: 'splash' }
];
