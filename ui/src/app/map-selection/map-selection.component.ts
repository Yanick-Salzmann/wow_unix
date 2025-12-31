import {ChangeDetectorRef, Component, OnInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {FormsModule} from '@angular/forms';
import {EventService} from '../service/event.service';
import {JsEventType, ListMapsResponseMap} from '../service/js-event';
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
                type: JsEventType.ListMapsRequest,
                list_maps_request_data: {}
            });

            if (response.type === JsEventType.ListMapsResponse) {
                this.maps = response.list_maps_response_data.maps;
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

    async selectMap(map: ListMapsResponseMap) {
        console.log('Selected map:', map);
        await this.router.navigate(['/minimap', map.map_id]);
    }

    hasFilteredMaps(): boolean {
        return this.$filteredMaps.value.length > 0;
    }
}
