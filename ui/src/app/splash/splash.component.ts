import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-splash',
  templateUrl: './splash.component.html',
  styleUrls: ['./splash.component.scss'],
  standalone: true,
  imports: [FormsModule, CommonModule]
})
export class SplashComponent {
  wowClientPath = '';

  handleFileSelection(event: Event) {
    event.preventDefault();
    const input = event.target as HTMLInputElement;
    if (input.files && input.files.length > 0) {
      const selectedFile = input.files[0];

      const folderName = selectedFile.webkitRelativePath.split('/')[0];
      this.wowClientPath = folderName;

      console.log('Selected folder:', folderName);
    }
  }

  loadData() {
    console.log('Loading data from path:', this.wowClientPath);
  }
}
