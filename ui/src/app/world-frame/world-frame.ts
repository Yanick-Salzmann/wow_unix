import { Component, OnInit } from '@angular/core';
import { EventService } from '../service/event.service';
import { CommonModule } from '@angular/common';
import { JsEvent } from '../proto/js_event';
import { BehaviorSubject } from 'rxjs';

interface AreaInfo {
  name: string;
  id: number;
}

interface WorldPosition {
  mapName: string;
  mapId: number;
  x: number;
  y: number;
  z: number;
}

@Component({
  selector: 'app-world-frame',
  imports: [CommonModule],
  templateUrl: './world-frame.html',
  styleUrl: './world-frame.scss',
  standalone: true
})
export class WorldFrame implements OnInit {
  protected areaInfo$ = new BehaviorSubject<AreaInfo | null>(null);
  protected worldPosition$ = new BehaviorSubject<WorldPosition | null>(null);

  constructor(private eventService: EventService) {}

  ngOnInit(): void {
    this.eventService.listenForEvent('areaUpdateEvent', (event: JsEvent) => {
      if (event.event.oneofKind === 'areaUpdateEvent') {
        this.areaInfo$.next({
          name: event.event.areaUpdateEvent.areaName,
          id: event.event.areaUpdateEvent.areaId
        });
      }
    });

    this.eventService.listenForEvent('worldPositionUpdateEvent', (event: JsEvent) => {
      if (event.event.oneofKind === 'worldPositionUpdateEvent') {
        this.worldPosition$.next({
          mapName: event.event.worldPositionUpdateEvent.mapName,
          mapId: event.event.worldPositionUpdateEvent.mapId,
          x: event.event.worldPositionUpdateEvent.x,
          y: event.event.worldPositionUpdateEvent.y,
          z: event.event.worldPositionUpdateEvent.z
        });
      }
    });
  }
}
