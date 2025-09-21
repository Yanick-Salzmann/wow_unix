import { Component, OnInit, OnDestroy, AfterViewInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ActivatedRoute, RouterModule } from '@angular/router';
import * as L from 'leaflet';

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

  constructor(private route: ActivatedRoute) {}

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
      center: [0, 0],
      zoom: 3,
      minZoom: 1,
      maxZoom: 7,
      attributionControl: false,
      zoomControl: false
    });

    L.control.zoom({
      position: 'bottomright'
    }).addTo(this.map);

    L.tileLayer(`minimap://localhost/${this.mapId}/{z}/{x}/{y}`, {
      tileSize: 256,
      updateWhenIdle: true
    }).addTo(this.map);
  }
}
