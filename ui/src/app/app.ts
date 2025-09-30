import {Component} from '@angular/core';
import {RouterOutlet} from '@angular/router';
import {EventService} from "./service/event.service";
import {LoadingScreenService} from "./service/loading-screen.service";

@Component({
    selector: 'app-root',
    standalone: true,
    imports: [RouterOutlet],
    templateUrl: './app.html',
    styleUrl: './app.scss'
})
export class App {
    constructor(eventService: EventService, loadingScreenService: LoadingScreenService) {
        eventService.initialize();
        loadingScreenService.initialize();
    }
}
