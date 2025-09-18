import { Component, ChangeDetectorRef, OnInit } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';
import { Router } from '@angular/router';
import { EventService } from '../service/event.service';

@Component({
  selector: 'app-splash',
  templateUrl: './splash.component.html',
  styleUrls: ['./splash.component.scss'],
  standalone: true,
  imports: [FormsModule, CommonModule]
})
export class SplashComponent implements OnInit {
  wowClientPath = '';
  private readonly STORAGE_KEY = 'wow-client-path';

  constructor(
    private eventService: EventService,
    private cdr: ChangeDetectorRef,
    private router: Router
  ) {}

  ngOnInit() {
    const savedPath = localStorage.getItem(this.STORAGE_KEY);
    if (savedPath) {
      this.wowClientPath = savedPath;
      console.log('Loaded saved path:', savedPath);
    }
  }

  async browseFolder() {
    try {
      const selectedPath = await this.eventService.browseFolder(
        'Select World of Warcraft Installation Folder',
        this.wowClientPath || '/home',
        [],
        false
      );

      if (selectedPath) {
        this.wowClientPath = selectedPath;
        this.cdr.detectChanges();
        console.log('Selected folder:', selectedPath);
      }
    } catch (error) {
      console.error('Error browsing folder:', error);
    }
  }

  loadData() {
    if (this.wowClientPath) {
      localStorage.setItem(this.STORAGE_KEY, this.wowClientPath);
      this.router.navigate(['/loading'], { queryParams: { path: this.wowClientPath } });
    }
  }
}
