import {Injectable} from '@angular/core';
import {BehaviorSubject} from 'rxjs';
import {EventService} from './event.service';
import {JsEvent} from '../proto/js_event';
import {Router} from '@angular/router';

@Injectable({providedIn: 'root'})
export class LoadingScreenService {
  public loadingScreenImageInfo$ = new BehaviorSubject<{ url: string, width: number, height: number } | null>(null);
  public loadingProgress$ = new BehaviorSubject<number>(0);

  constructor(private eventService: EventService, private router: Router) {
  }

  public initialize() {
    console.log("Loading screen service initialized");

    this.eventService.listenForEvent("loadingScreenShowEvent", (event: JsEvent) => {
      if (event.event.oneofKind !== "loadingScreenShowEvent") {
        console.warn("Received wrong event type for loading screen: ", event);
        return;
      }
      this.setLoadingScreenImage(event.event.loadingScreenShowEvent.imagePath);
    });

    this.eventService.listenForEvent("loadingScreenProgressEvent", (event: JsEvent) => {
      if (event.event.oneofKind !== "loadingScreenProgressEvent") {
        console.warn("Received wrong event type for loading screen progress: ", event);
        return;
      }
      this.loadingProgress$.next(event.event.loadingScreenProgressEvent.percentage);
    });

    this.eventService.listenForEvent("loadingScreenCompleteEvent", (event: JsEvent) => {
      if (event.event.oneofKind !== "loadingScreenCompleteEvent") {
        console.warn("Received wrong event type for loading screen complete: ", event);
        return;
      }
      this.router.navigate(['/world-frame']);
    });
  }

  private setLoadingScreenImage(imageUrl: string) {
    const img = new Image();
    img.onload = () => {
      this.loadingScreenImageInfo$.next({
        url: imageUrl,
        width: img.naturalWidth,
        height: img.naturalHeight,
      });
    };
    img.src = imageUrl;
  }
}
