import {Injectable} from '@angular/core';

export interface MapState {
    center: {lat: number; lng: number};
    zoom: number;
}

@Injectable({
    providedIn: 'root'
})
export class MapStateService {
    private readonly STORAGE_KEY = 'wow-unix-map-states';

    saveMapState(mapId: string, state: MapState): void {
        const states = this.getAllMapStates();
        states[mapId] = state;
        localStorage.setItem(this.STORAGE_KEY, JSON.stringify(states));
    }

    getMapState(mapId: string): MapState | null {
        const states = this.getAllMapStates();
        return states[mapId] || null;
    }

    private getAllMapStates(): Record<string, MapState> {
        const stored = localStorage.getItem(this.STORAGE_KEY);
        return stored ? JSON.parse(stored) : {};
    }
}
