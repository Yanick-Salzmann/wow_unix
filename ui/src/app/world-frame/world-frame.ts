import { Component, OnInit, AfterViewInit, ViewChild, ElementRef } from '@angular/core';
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

interface SystemStats {
  cpu: number;
  memory: number;
  totalMemory: number;
  gpu: number;
  time: number;
}

@Component({
  selector: 'app-world-frame',
  imports: [CommonModule],
  templateUrl: './world-frame.html',
  styleUrl: './world-frame.scss',
  standalone: true
})
export class WorldFrame implements OnInit, AfterViewInit {
  protected areaInfo$ = new BehaviorSubject<AreaInfo | null>(null);
  protected worldPosition$ = new BehaviorSubject<WorldPosition | null>(null);
  protected fps$ = new BehaviorSubject<number>(0);

  @ViewChild('canvas', { static: true }) canvas!: ElementRef<HTMLCanvasElement>;

  private systemStatsHistory: SystemStats[] = [];
  private maxHistory = 500;
  private memoryScale = 1024 * 1024 * 100;
  private memoryScaleInitialized = false;

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

    this.eventService.listenForEvent('fpsUpdateEvent', (event: JsEvent) => {
      if (event.event.oneofKind === 'fpsUpdateEvent') {
        this.fps$.next(event.event.fpsUpdateEvent.fps);
      }
    });

    this.eventService.listenForEvent('systemUpdateEvent', (event: JsEvent) => {
      if (event.event.oneofKind === 'systemUpdateEvent') {
        const stats: SystemStats = {
          cpu: event.event.systemUpdateEvent.cpuUsage,
          memory: Number(event.event.systemUpdateEvent.memoryUsage),
          totalMemory: Number(event.event.systemUpdateEvent.totalMemory),
          gpu: event.event.systemUpdateEvent.gpuUsage,
          time: Date.now()
        };
        this.systemStatsHistory.push(stats);
        if (this.systemStatsHistory.length > this.maxHistory) {
          this.systemStatsHistory.shift();
        }
        this.drawGraph();
      }
    });
  }

  ngAfterViewInit(): void {
    const canvas = this.canvas.nativeElement;
    canvas.width = 300;
    canvas.height = 150;
    const ctx = canvas.getContext('2d');
    if (ctx) {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
    }
  }

  private getNextMemoryScale(currentMax: number): number {
    const baseScales = [
      10 * 1024,                      // 10KB
      100 * 1024,                     // 100KB
      1024 * 1024,                    // 1MB
      10 * 1024 * 1024,               // 10MB
      100 * 1024 * 1024,              // 100MB
      1024 * 1024 * 1024,             // 1GB
      10 * 1024 * 1024 * 1024,        // 10GB
      100 * 1024 * 1024 * 1024,       // 100GB
    ];

    if (!this.memoryScaleInitialized) {
      for (const baseScale of baseScales) {
        if (currentMax <= baseScale * 0.8) {
          this.memoryScale = baseScale;
          this.memoryScaleInitialized = true;
          return this.memoryScale;
        }
      }
      this.memoryScale = baseScales[baseScales.length - 1];
      this.memoryScaleInitialized = true;
      return this.memoryScale;
    }

    while (currentMax > this.memoryScale * 0.8) {
      this.memoryScale = Math.ceil(this.memoryScale * 1.5);
    }

    return this.memoryScale;
  }

  private drawGraph(): void {
    const canvas = this.canvas.nativeElement;
    const ctx = canvas.getContext('2d');
    if (!ctx || this.systemStatsHistory.length === 0) return;

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    const width = canvas.width;
    const height = canvas.height;
    const len = this.systemStatsHistory.length;

    const maxMemory = Math.max(...this.systemStatsHistory.map(s => s.memory));
    this.memoryScale = this.getNextMemoryScale(maxMemory);

    const data = this.systemStatsHistory.map(s => ({
      cpu: s.cpu,
      mem: (s.memory / this.memoryScale) * 100,
      memAbsolute: s.memory,
      totalMem: s.totalMemory,
      gpu: s.gpu
    }));

    const scaleX = width / (len - 1);
    const scaleY = height / 100;

    ctx.strokeStyle = 'red';
    ctx.lineWidth = 2;
    ctx.beginPath();
    for (let i = 0; i < len; i++) {
      const x = i * scaleX;
      const y = height - data[i].cpu * scaleY;
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();

    ctx.strokeStyle = 'blue';
    ctx.lineWidth = 2;
    ctx.beginPath();
    for (let i = 0; i < len; i++) {
      const x = i * scaleX;
      const y = height - Math.min(data[i].mem, 100) * scaleY;
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();

    ctx.strokeStyle = 'green';
    ctx.lineWidth = 2;
    ctx.beginPath();
    for (let i = 0; i < len; i++) {
      const x = i * scaleX;
      const y = height - data[i].gpu * scaleY;
      if (i === 0) ctx.moveTo(x, y);
      else ctx.lineTo(x, y);
    }
    ctx.stroke();

    const current = data[len - 1];

    const formatBytes = (bytes: number): string => {
      const gb = bytes / (1024 * 1024 * 1024);
      if (gb >= 1) return `${gb.toFixed(2)}GB`;
      const mb = bytes / (1024 * 1024);
      if (mb >= 1) return `${mb.toFixed(2)}MB`;
      const kb = bytes / 1024;
      return `${kb.toFixed(2)}KB`;
    };

    ctx.font = '13px monospace';
    ctx.lineWidth = 3;
    ctx.lineJoin = 'round';

    ctx.fillStyle = 'red';
    ctx.fillRect(10, 5, 12, 12);
    ctx.strokeStyle = 'black';
    ctx.strokeText(`CPU: ${current.cpu}%`, 26, 16);
    ctx.fillStyle = 'white';
    ctx.fillText(`CPU: ${current.cpu}%`, 26, 16);

    ctx.fillStyle = 'blue';
    ctx.fillRect(10, 22, 12, 12);
    ctx.strokeStyle = 'black';
    ctx.strokeText(`Mem: ${formatBytes(current.memAbsolute)} / ${formatBytes(this.memoryScale)} (${Math.min(current.mem, 100).toFixed(1)}%)`, 26, 33);
    ctx.fillStyle = 'white';
    ctx.fillText(`Mem: ${formatBytes(current.memAbsolute)} / ${formatBytes(this.memoryScale)} (${Math.min(current.mem, 100).toFixed(1)}%)`, 26, 33);

    ctx.fillStyle = 'green';
    ctx.fillRect(10, 39, 12, 12);
    ctx.strokeStyle = 'black';
    ctx.strokeText(`GPU: ${current.gpu}%`, 26, 50);
    ctx.fillStyle = 'white';
    ctx.fillText(`GPU: ${current.gpu}%`, 26, 50);
  }
}
