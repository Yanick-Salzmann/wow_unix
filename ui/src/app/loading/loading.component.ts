import {Component, OnInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {Router, ActivatedRoute} from '@angular/router';
import {EventService} from '../service/event.service';
import {BehaviorSubject} from "rxjs";
import {JsEventType} from "../service/js-event";

@Component({
    selector: 'app-loading',
    templateUrl: './loading.component.html',
    styleUrls: ['./loading.component.scss'],
    standalone: true,
    imports: [CommonModule]
})
export class LoadingComponent implements OnInit {
    $loadingProgress = new BehaviorSubject<number>(0);
    $loadingText = new BehaviorSubject<string>('Initializing...');

    wowClientPath = '';

    constructor(
        private eventService: EventService,
        private router: Router,
        private route: ActivatedRoute
    ) {

    }

    async ngOnInit() {
        const path = this.route.snapshot.queryParamMap.get('path');
        if (path) {
            this.wowClientPath = path;
        } else {
            throw new Error('WoW client path not provided');
        }

        console.log('Loading data from path:', this.wowClientPath);
        this.eventService.listenForEvent(JsEventType.LoadUpdateEvent, (event) => {
            if (event.type !== JsEventType.LoadUpdateEvent) {
                return;
            }

            this.$loadingProgress.next(event.load_update_event_data.percentage);
            this.$loadingText.next(event.load_update_event_data.message);

            if (this.$loadingProgress.value >= 100) {
                setTimeout(() => {
                    this.router.navigate(['/map-selection']);
                }, 500);
            }
        })

        await this.eventService.sendMessage({
                type: JsEventType.LoadDataEvent,
                load_data_event_data: {
                    folder: this.wowClientPath
                }
        })
    }
}
