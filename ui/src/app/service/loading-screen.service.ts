import {Injectable} from '@angular/core';
import {BehaviorSubject} from 'rxjs';
import {EventService} from './event.service';
import {JsEvent, JsEventType} from './js-event';
import {Router} from '@angular/router';

@Injectable({providedIn: 'root'})
export class LoadingScreenService {
  public loadingScreenImageInfo$ = new BehaviorSubject<{ url: string, width: number, height: number } | null>(null);
  public loadingProgress$ = new BehaviorSubject<number>(0);

  constructor(private eventService: EventService, private router: Router) {
  }

  public initialize() {
    console.log("Loading screen service initialized");

    this.eventService.listenForEvent(JsEventType.LoadingScreenShowEvent, (event: JsEvent) => {
      if (event.type !== JsEventType.LoadingScreenShowEvent) {
        console.warn("Received wrong event type for loading screen: ", event);
        return;
      }
      this.setLoadingScreenImage(event.loading_screen_show_event_data.image_path);
    });

    this.eventService.listenForEvent(JsEventType.LoadingScreenProgressEvent, (event: JsEvent) => {
      if (event.type !== JsEventType.LoadingScreenProgressEvent) {
        console.warn("Received wrong event type for loading screen progress: ", event);
        return;
      }
      this.loadingProgress$.next(event.loading_screen_progress_event_data.percentage);
    });

    this.eventService.listenForEvent(JsEventType.LoadingScreenCompleteEvent, async (event: JsEvent) => {
      if (event.type !== JsEventType.LoadingScreenCompleteEvent) {
        console.warn("Received wrong event type for loading screen complete: ", event);
        return;
      }

      await this.router.navigate(['/world-frame']);
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
