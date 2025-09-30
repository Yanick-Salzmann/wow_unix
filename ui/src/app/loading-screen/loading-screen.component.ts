import {Component} from '@angular/core';
import {BehaviorSubject} from "rxjs";
import {AsyncPipe, NgOptimizedImage} from "@angular/common";
import {LoadingScreenService} from "../service/loading-screen.service";

@Component({
  selector: 'app-loading-screen',
  standalone: true,
  imports: [
    AsyncPipe,
    NgOptimizedImage
  ],
  templateUrl: './loading-screen.component.html',
  styleUrls: ['./loading-screen.component.scss']
})
export class LoadingScreenComponent {
  public loadingScreenImageInfo$: BehaviorSubject<{ url: string; width: number; height: number } | null>;
  public loadingProgress$: BehaviorSubject<number>;

  constructor(loadingScreenService: LoadingScreenService) {
    this.loadingScreenImageInfo$ = loadingScreenService.loadingScreenImageInfo$;
    this.loadingProgress$ = loadingScreenService.loadingProgress$;
  }
}
