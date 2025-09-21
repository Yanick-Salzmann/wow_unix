import {ChangeDetectionStrategy, ChangeDetectorRef, Component, OnInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {FormsModule} from '@angular/forms';
import {EventService} from '../service/event.service';
import {ListMapsResponseMap} from '../proto/js_event';
import {Router} from '@angular/router';
import {BehaviorSubject} from "rxjs";

@Component({
    selector: 'app-map-selection',
    templateUrl: './map-selection.component.html',
    styleUrls: ['./map-selection.component.scss'],
    standalone: true,
    imports: [CommonModule, FormsModule]
})
export class MapSelectionComponent implements OnInit {
    $filteredMaps = new BehaviorSubject<ListMapsResponseMap[]>([]);
    $isLoading = new BehaviorSubject<boolean>(true);

    maps: ListMapsResponseMap[] = [];

    filterText = '';

    constructor(
        private eventService: EventService,
        private router: Router,
        private cdr: ChangeDetectorRef
    ) {
    }

    ngOnInit() {
        this.loadMaps().then(_ => {
        });
    }

    async loadMaps() {
        this.$isLoading.next(true);

        try {
            const response = await this.eventService.sendMessageWithResponse({
                event: {
                    oneofKind: 'listMapsRequest',
                    listMapsRequest: {}
                }
            });

            if (response?.event?.oneofKind === 'listMapsResponse') {
                this.maps = response.event.listMapsResponse.maps;
                this.$filteredMaps.next([...this.maps]);
                this.$isLoading.next(false);
            }
        } catch (error) {
            console.error('Failed to load maps:', error);
            this.$isLoading.next(false);
        }
    }

    filterMaps() {
        if (!this.filterText) {
            this.$filteredMaps.next([...this.maps]);
            return;
        }

        const searchTerm = this.filterText.toLowerCase();
        this.$filteredMaps.next(this.maps.filter(map =>
            map.name.toLowerCase().includes(searchTerm)
        ));
    }

    selectMap(map: ListMapsResponseMap) {
        console.log('Selected map:', map);
        this.router.navigate(['/minimap', map.mapId]);
    }

    hasFilteredMaps(): boolean {
        return this.$filteredMaps.value.length > 0;
    }
}
