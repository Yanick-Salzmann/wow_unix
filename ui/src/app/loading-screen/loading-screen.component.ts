import {Component} from '@angular/core';
import {EventService} from "../service/event.service";
import {BehaviorSubject} from "rxjs";

@Component({
    selector: 'app-loading-screen',
    standalone: true,
    templateUrl: './loading-screen.component.html',
    styleUrls: ['./loading-screen.component.scss']
})
export class LoadingScreenComponent {
    private loadingScreenImage$ = new BehaviorSubject<string>("");

    constructor(eventService: EventService) {
        eventService.listenForEvent("loadingScreenShowEvent", async (event) => {
            if (event.event.oneofKind != "loadingScreenShowEvent") {
                console.warn("Received wrong event type in LoadingScreenComponent: ", event);
                return;
            }

            this.loadingScreenImage$.next(event.event.loadingScreenShowEvent.imagePath);
        });
    }
}
