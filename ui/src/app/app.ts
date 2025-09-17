import {Component} from '@angular/core';
import {RouterOutlet} from '@angular/router';
import {EventService} from "./service/event.service";
import {JsEvent} from "./proto/js_event";

@Component({
    selector: 'app-root',
    standalone: true,
    imports: [RouterOutlet],
    templateUrl: './app.html',
    styleUrl: './app.scss'
})
export class App {
    constructor(eventService: EventService) {
        eventService.initialize();
        eventService.browseFolder("Select Wow Client Folder", "", [], false).then(path => {
            console.log("Selected path: " + path);
        })
    }
}
