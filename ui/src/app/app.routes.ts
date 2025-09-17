import { Routes } from '@angular/router';
import { SplashComponent } from './splash/splash.component';
import { LoadingComponent } from './loading/loading.component';

export const routes: Routes = [
  { path: 'splash', component: SplashComponent },
  { path: 'loading', component: LoadingComponent },
  { path: '', redirectTo: 'splash', pathMatch: 'full' },
  { path: '**', redirectTo: 'splash' }
];
