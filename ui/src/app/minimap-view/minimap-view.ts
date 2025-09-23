import {Component, OnInit, OnDestroy, AfterViewInit} from '@angular/core';
import {CommonModule} from '@angular/common';
import {ActivatedRoute, RouterModule} from '@angular/router';
import {BehaviorSubject} from 'rxjs';
import * as L from 'leaflet';
import {EventService} from '../service/event.service';
import {MapPoi} from '../proto/js_event';

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
    private markers: L.Marker[] = [];
    mouseCoordinates$ = new BehaviorSubject<{ x: number; y: number } | null>(null);

    constructor(
        private route: ActivatedRoute,
        private eventService: EventService
    ) {
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

        this.map.on('mousemove', (e: L.LeafletMouseEvent) => {
            const point = this.map!.project(e.latlng, this.map!.getMaxZoom());
            const x = Math.round(point.x * mapSize / tileSize);
            const y = Math.round(point.y * mapSize / tileSize);

            this.mouseCoordinates$.next({
                x: Math.max(0, Math.min(mapSize, x)),
                y: Math.max(0, Math.min(mapSize, y))
            });
        });

        this.map.on('mouseout', () => {
            this.mouseCoordinates$.next(null);
        });

        this.loadMapPois();
    }

    private async loadMapPois(): Promise<void> {
        try {
            const response = await this.eventService.sendMessageWithResponse({
                event: {
                    oneofKind: 'listMapPoisRequest',
                    listMapPoisRequest: {
                        mapId: parseInt(this.mapId)
                    }
                }
            });

            if (response?.event?.oneofKind === 'listMapPoisResponse') {
                this.addPoisAsMarkers(response.event.listMapPoisResponse.pois);
            }
        } catch (error) {
            console.error('Failed to load map POIs:', error);
        }
    }

    private addPoisAsMarkers(pois: MapPoi[]): void {
        if (!this.map) return;

        this.clearMarkers();

        const icon = L.icon({
            iconUrl: 'blp://localhost/Interface/WorldStateFrame/HordeFlag.blp',
            iconSize: [32, 32],
        })

        pois.forEach(poi => {
            const worldX = (halfMapSize - poi.y) * tileSize / mapSize;
            const worldY = (halfMapSize - poi.x) * tileSize / mapSize;

            const latlng = this.map!.unproject([worldX, worldY], this.map!.getMaxZoom());

            const popupContent = `
                <div class="poi-popup">
                    <h3 class="poi-name">${poi.name}</h3>
                    <button class="enter-world-btn" onclick="window.enterWorld('${poi.name}', ${poi.x}, ${poi.y})">
                        ✨ Enter World ✨
                    </button>
                </div>
            `;

            const marker = L.marker(latlng)
                .setIcon(icon)
                .bindPopup(popupContent, {
                    className: 'custom-popup',
                    closeButton: true,
                    maxWidth: 200
                })
                .addTo(this.map!);

            this.markers.push(marker);
        });

        (window as any).enterWorld = (poiName: string, x: number, y: number) => {
            this.enterWorld(poiName, x, y);
        };
    }

    private clearMarkers(): void {
        this.markers.forEach(marker => {
            if (this.map) {
                this.map.removeLayer(marker);
            }
        });
        this.markers = [];
    }

    ngOnDestroy(): void {
        this.clearMarkers();
        if (this.map) {
            this.map.remove();
        }
    }

    private enterWorld(poiName: string, x: number, y: number): void {
        console.log(`Entering world at ${poiName} (${x}, ${y})`);

        const button = document.querySelector('.enter-world-btn') as HTMLButtonElement;
        if (button) {
            button.classList.add('entering');
            button.innerHTML = '🌟 Entering... 🌟';
            button.disabled = true;
        }

        setTimeout(() => {
            alert(`Welcome to ${poiName}!\nYou have entered the world at coordinates (${x}, ${y})`);

            if (button) {
                button.classList.remove('entering');
                button.innerHTML = '✨ Enter World ✨';
                button.disabled = false;
            }
        }, 2000);
    }
}
