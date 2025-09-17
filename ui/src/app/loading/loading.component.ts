import { Component, OnInit, OnDestroy } from '@angular/core';
import { CommonModule } from '@angular/common';
import { Router } from '@angular/router';
import { EventService } from '../service/event.service';

@Component({
  selector: 'app-loading',
  templateUrl: './loading.component.html',
  styleUrls: ['./loading.component.scss'],
  standalone: true,
  imports: [CommonModule]
})
export class LoadingComponent implements OnInit, OnDestroy {
  loadingProgress = 0;
  loadingText = 'Initializing...';
  dots = '';
  private dotsInterval: any;
  private progressInterval: any;

  constructor(
    private eventService: EventService,
    private router: Router
  ) {}

  ngOnInit() {
    this.startLoadingAnimation();
    this.simulateLoading();
  }

  ngOnDestroy() {
    if (this.dotsInterval) {
      clearInterval(this.dotsInterval);
    }
    if (this.progressInterval) {
      clearInterval(this.progressInterval);
    }
  }

  private startLoadingAnimation() {
    this.dotsInterval = setInterval(() => {
      if (this.dots.length >= 3) {
        this.dots = '';
      } else {
        this.dots += '.';
      }
    }, 500);
  }

  private simulateLoading() {
    const loadingSteps = [
      { progress: 20, text: 'Loading WoW client data' },
      { progress: 40, text: 'Parsing game files' },
      { progress: 60, text: 'Initializing database' },
      { progress: 80, text: 'Setting up environment' },
      { progress: 100, text: 'Ready to launch' }
    ];

    let currentStep = 0;

    this.progressInterval = setInterval(() => {
      if (currentStep < loadingSteps.length) {
        this.loadingProgress = loadingSteps[currentStep].progress;
        this.loadingText = loadingSteps[currentStep].text;
        currentStep++;
      } else {
        clearInterval(this.progressInterval);
        setTimeout(() => {
          // Navigate to the main application or game view
          // For now, we'll just stay on loading screen
          console.log('Loading complete!');
        }, 1000);
      }
    }, 1500);
  }
}
