import {Component, OnInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {Router, ActivatedRoute} from '@angular/router';
import {EventService} from '../service/event.service';
import {BehaviorSubject} from "rxjs";

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
        this.eventService.listenForEvent("loadUpdateEvent", (event) => {
            if (event.event.oneofKind !== "loadUpdateEvent") {
                return;
            }

            this.$loadingProgress.next(event.event.loadUpdateEvent.percentage);
            this.$loadingText.next(event.event.loadUpdateEvent.message);

            if (this.$loadingProgress.value >= 100) {
                setTimeout(() => {
                    this.router.navigate(['/map-selection']);
                }, 500);
            }
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
}
