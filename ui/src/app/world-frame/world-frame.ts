import {AfterViewInit, Component, ElementRef, OnDestroy, OnInit, ViewChild} from '@angular/core';
import {EventService} from '../service/event.service';
import {CommonModule} from '@angular/common';
import {JsEvent, JsEventType} from '../service/js-event';
import {BehaviorSubject} from 'rxjs';
import {LocaleNumberPipe} from '../pipes/locale-number.pipe';

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
    cpuFreq: number;
    gpuMemUsed: number;
    gpuMemTotal: number;
}

@Component({
    selector: 'app-world-frame',
    imports: [CommonModule, LocaleNumberPipe],
    templateUrl: './world-frame.html',
    styleUrl: './world-frame.scss',
    standalone: true
})
export class WorldFrame implements OnInit, AfterViewInit, OnDestroy {
    protected areaInfo$ = new BehaviorSubject<AreaInfo | null>(null);
    protected worldPosition$ = new BehaviorSubject<WorldPosition | null>(null);
    protected fps$ = new BehaviorSubject<number>(0);
    protected timeOfDay$ = new BehaviorSubject<string>('00:00');
    protected currentStats$ = new BehaviorSubject<SystemStats | null>(null);
    protected currentSound$ = new BehaviorSubject<string | null>(null);

    @ViewChild('canvas', {static: true}) canvas!: ElementRef<HTMLCanvasElement>;

    private systemStatsHistory: SystemStats[] = [];
    private maxHistory = 100;
    private memoryScale = 1024 * 1024 * 100;
    private memoryScaleInitialized = false;

    private animationFrameId: number | null = null;

    constructor(private eventService: EventService) {
    }

    async ngOnInit(): Promise<void> {
        this.eventService.listenForEvent(JsEventType.AreaUpdateEvent, (event: JsEvent) => {
            if (event.type === JsEventType.AreaUpdateEvent) {
                this.areaInfo$.next({
                    name: event.area_update_event_data.area_name,
                    id: event.area_update_event_data.area_id
                });
            }
        });

        this.eventService.listenForEvent(JsEventType.WorldPositionUpdateEvent, (event: JsEvent) => {
            if (event.type === JsEventType.WorldPositionUpdateEvent) {
                this.worldPosition$.next({
                    mapName: event.world_position_update_event_data.map_name,
                    mapId: event.world_position_update_event_data.map_id,
                    x: event.world_position_update_event_data.x,
                    y: event.world_position_update_event_data.y,
                    z: event.world_position_update_event_data.z
                });
            }
        });

        this.eventService.listenForEvent(JsEventType.FpsUpdateEvent, async (event: JsEvent) => {
            if (event.type === JsEventType.FpsUpdateEvent) {
                this.fps$.next(event.fps_update_event_data.fps);
            }
        });

        await this.updateTime();

        this.eventService.listenForEvent(JsEventType.SystemUpdateEvent, (event: JsEvent) => {
            if (event.type === JsEventType.SystemUpdateEvent) {
                const stats: SystemStats = {
                    cpu: Number(event.system_update_event_data.cpu_usage) || 0,
                    memory: Number(event.system_update_event_data.memory_usage) || 0,
                    totalMemory: Number(event.system_update_event_data.total_memory) || 0,
                    gpu: Number(event.system_update_event_data.gpu_usage) || 0,
                    time: Date.now(),
                    cpuFreq: Number(event.system_update_event_data.cpu_frequency_mhz) || 0,
                    gpuMemUsed: Number(event.system_update_event_data.gpu_memory_used) || 0,
                    gpuMemTotal: Number(event.system_update_event_data.gpu_memory_total) || 0
                };

                this.currentStats$.next(stats);

                this.systemStatsHistory.push(stats);
                if (this.systemStatsHistory.length > this.maxHistory) {
                    this.systemStatsHistory.shift();
                }
                requestAnimationFrame(() => this.drawGraph());
            }
        });

        this.eventService.listenForEvent(JsEventType.SoundUpdateEvent, (event: JsEvent) => {
            if (event.type === JsEventType.SoundUpdateEvent) {
                this.currentSound$.next(event.sound_update_event_data.sound_name || null);
            }
        });
    }

    ngOnDestroy(): void {
        if (this.animationFrameId !== null) {
            cancelAnimationFrame(this.animationFrameId);
            this.animationFrameId = null;
        }
    }

    private async updateTime() {
        const resp = await this.eventService.sendMessageWithResponse({
                type: JsEventType.FetchGameTimeRequest,
                fetch_game_time_request_data: {}
            }
        );

        if (resp.type !== JsEventType.FetchGameTimeResponse) {
            return;
        }

        const timeOfDay = resp.fetch_game_time_response_data.time_of_day

        const dt = new Date(Number(timeOfDay) * 1000);
        this.timeOfDay$.next(dt.toLocaleTimeString([], {hour: '2-digit', minute: '2-digit'}));

        setTimeout(() => this.updateTime(), 200);
    }

    ngAfterViewInit(): void {
        const canvas = this.canvas.nativeElement;
        canvas.width = 300;
        canvas.height = 100; // Reduced height
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

        const width = canvas.width;
        const height = canvas.height;

        ctx.clearRect(0, 0, width, height);

        const len = this.systemStatsHistory.length;
        if (len < 2) return;

        const maxMemory = Math.max(...this.systemStatsHistory.map(s => s.memory));
        this.memoryScale = this.getNextMemoryScale(maxMemory);

        const data = this.systemStatsHistory.map(s => ({
            cpu: s.cpu,
            mem: (s.memory / this.memoryScale) * 100, // Normalized to 0-100 relative to scale
            gpu: s.gpu
        }));

        // Helper to draw smooth path
        const drawPath = (getValue: (i: number) => number, color: string, gradientColor: string) => {
            ctx.beginPath();

            // Start from result of (maxHistory - len) * stepX to keep right alignment?
            // Actually, just drawing left to right for now, but to keep "scrolling" look,
            // we usually want fixed window.
            // Let's draw the current history spread over the width.
            // Actually for a rolling graph, we might want the rightmost point to be "now".

            ctx.moveTo(0, height - (getValue(0) / 100) * height);

            for (let i = 1; i < len; i++) {
                const x = i * (width / (len - 1));
                const y = height - (getValue(i) / 100) * height;
                // Simple line for now, curves can be tricky with fluctuating data
                // but let's try a simple quadratic or just line to keep it performant and sharp
                ctx.lineTo(x, y);
            }

            ctx.strokeStyle = color;
            ctx.lineWidth = 2;
            ctx.stroke();

            // Gradient fill
            ctx.lineTo(width, height);
            ctx.lineTo(0, height);
            ctx.closePath();

            const gradient = ctx.createLinearGradient(0, 0, 0, height);
            gradient.addColorStop(0, gradientColor);
            gradient.addColorStop(1, "transparent");

            ctx.fillStyle = gradient;
            ctx.fill();
        };

        // Draw layers (GPU bottom, Memory middle, CPU top usually good, or just overlay)
        // Using standard colors but brighter

        // GPU - Green
        drawPath(i => data[i].gpu, '#00ff00', 'rgba(0, 255, 0, 0.2)');

        // Memory - Blue
        drawPath(i => data[i].mem, '#0088ff', 'rgba(0, 136, 255, 0.2)');

        // CPU - Red
        drawPath(i => data[i].cpu, '#ff3333', 'rgba(255, 51, 51, 0.2)');
    }
}
