import {Component, OnInit, OnDestroy, ChangeDetectorRef} from '@angular/core';
import {CommonModule} from '@angular/common';
import {Router, ActivatedRoute} from '@angular/router';
import {EventService} from '../service/event.service';
import {JsEvent, LoadUpdateEvent} from "../proto/js_event";

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
    private progressInterval: any;
    wowClientPath = '';

    constructor(
        private eventService: EventService,
        private router: Router,
        private cdr: ChangeDetectorRef,
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
        this.eventService.listenForEvent("loadUpdateEvent", (event) => {
            if(event.event.oneofKind !== "loadUpdateEvent") {
                return;
            }

            this.loadingProgress = event.event.loadUpdateEvent.percentage;
            this.loadingText = event.event.loadUpdateEvent.message;
            this.cdr.detectChanges();
        })

        await this.eventService.sendMessage({
            event: {
                oneofKind: "loadDataEvent",
                loadDataEvent: {
                    folder: this.wowClientPath
                }
            }
        })
    }

    ngOnDestroy() {
        if (this.progressInterval) {
            clearInterval(this.progressInterval);
        }
    }
}
