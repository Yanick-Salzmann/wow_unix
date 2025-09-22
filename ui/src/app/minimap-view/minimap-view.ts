import {Component, OnInit, OnDestroy, AfterViewInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {ActivatedRoute, RouterModule} from '@angular/router';
import * as L from 'leaflet';

const mapSize = 64 * (533.0 + 1.0 / 3.0);
const tileSize = 64 * 256;
const halfMapSize = 32 * (533.0 + 1.0 / 3.0);

@Component({
    selector: 'app-minimap-view',
    standalone: true,
    imports: [CommonModule, RouterModule],
    templateUrl: './minimap-view.html',
    styleUrls: ['./minimap-view.scss']
})
export class MinimapViewComponent implements OnInit, AfterViewInit, OnDestroy {
    private map: L.Map | null = null;
    mapId: string = '';
    private mapInitialized = false;

    constructor(private route: ActivatedRoute) {
    }

    ngOnInit(): void {
        this.route.paramMap.subscribe(params => {
            this.mapId = params.get('mapId') || '';
        });
    }

    ngAfterViewInit(): void {
        setTimeout(() => {
            if (this.mapId && !this.mapInitialized) {
                this.initMap();
                this.mapInitialized = true;
            }
        }, 100);
    }

    ngOnDestroy(): void {
        if (this.map) {
            this.map.remove();
        }
    }

    private initMap(): void {
        if (!this.mapId) return;

        this.map = L.map('map-container', {
            minZoom: 3,
            maxZoom: 6,
            crs: L.CRS.Simple,
        });

        L.tileLayer(`minimap://localhost/${this.mapId}/{z}/{x}/{y}`, {
            minZoom: 3,
            maxZoom: 6,
        }).addTo(this.map);

        const sw = this.map.unproject([0, tileSize], this.map.getMaxZoom());
        const ne = this.map.unproject([tileSize, 0], this.map.getMaxZoom());

        this.map.setMaxBounds(new L.LatLngBounds(sw, ne));
        this.map.setView(this.map.unproject([tileSize / 2, tileSize / 2], this.map.getMaxZoom()), 6);

    }
}
