import {Component, OnInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {FormsModule} from '@angular/forms';
import {EventService} from '../service/event.service';
import {ListMapsResponseMap} from '../proto/js_event';
import {Router} from '@angular/router';

@Component({
    selector: 'app-map-selection',
    templateUrl: './map-selection.component.html',
    styleUrls: ['./map-selection.component.scss'],
    standalone: true,
    imports: [CommonModule, FormsModule]
})
export class MapSelectionComponent implements OnInit {
    maps: ListMapsResponseMap[] = [];
    filteredMaps: ListMapsResponseMap[] = [];
    filterText = '';
    isLoading = true;

    constructor(
        private eventService: EventService,
        private router: Router
    ) {
    }

    ngOnInit() {
        this.loadMaps().then(_ => {
        });
    }

    async loadMaps() {
        this.isLoading = true;

        try {
            const response = await this.eventService.sendMessageWithResponse({
                event: {
                    oneofKind: 'listMapsRequest',
                    listMapsRequest: {}
                }
            });

            if (response?.event?.oneofKind === 'listMapsResponse') {
                this.maps = response.event.listMapsResponse.maps;
                this.filteredMaps = [...this.maps];
                this.isLoading = false;
            }
        } catch (error) {
            console.error('Failed to load maps:', error);
            this.isLoading = false;
        }
    }

    filterMaps() {
        if (!this.filterText) {
            this.filteredMaps = [...this.maps];
            return;
        }

        const searchTerm = this.filterText.toLowerCase();
        this.filteredMaps = this.maps.filter(map =>
            map.name.toLowerCase().includes(searchTerm)
        );
    }

    selectMap(map: ListMapsResponseMap) {
        console.log('Selected map:', map);
    }
}
