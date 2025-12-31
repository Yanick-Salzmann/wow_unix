export enum JsEventType {
    None = 0,
    InitializeRequest = 1,
    BrowseFolderRequest = 2,
    BrowseFolderResponse = 3,
    EmptyResponse = 4,
    LoadDataEvent = 5,
    LoadUpdateEvent = 6,
    ListMapsRequest = 7,
    ListMapsResponse = 8,
    ListMapPoisRequest = 9,
    ListMapPoisResponse = 10,
    EnterWorldRequest = 11,
    LoadingScreenShowEvent = 12,
    LoadingScreenProgressEvent = 13,
    LoadingScreenCompleteEvent = 14,
    AreaUpdateEvent = 15,
    WorldPositionUpdateEvent = 16,
    FpsUpdateEvent = 17,
    SystemUpdateEvent = 18,
    FetchGameTimeRequest = 19,
    FetchGameTimeResponse = 20,
    SoundUpdateEvent = 21
}

export interface InitializeRequest {}
export interface BrowseFolderRequest { title: string; default_path: string; filters: string[]; allow_create: boolean; }
export interface BrowseFolderResponse { cancelled: boolean; path: string; }
export interface EmptyResponse {}
export interface LoadDataEvent { folder: string; }
export interface LoadUpdateEvent { percentage: number; completed: boolean; message: string; }
export interface ListMapsRequest {}
export interface ListMapsResponseMap { map_id: number; name: string; loading_screen: string; }
export interface ListMapsResponse { maps: ListMapsResponseMap[]; }
export interface ListMapPoisRequest { map_id: number; }
export interface MapPoi { id: number; name: string; x: number; y: number; }
export interface ListMapPoisResponse { map_id: number; pois: MapPoi[]; }
export interface EnterWorldRequest { map_id: number; x: number; y: number; }
export interface LoadingScreenShowEvent { image_path: string; }
export interface LoadingScreenProgressEvent { percentage: number; }
export interface LoadingScreenCompleteEvent {}
export interface AreaUpdateEvent { area_id: number; area_name: string; }
export interface WorldPositionUpdateEvent { map_id: number; map_name: string; x: number; y: number; z: number; }
export interface FpsUpdateEvent { fps: number; time_of_day: number; }
export interface SystemUpdateEvent { memory_usage: number; cpu_usage: number; gpu_usage: number; total_memory: number; cpu_frequency_mhz: number; gpu_memory_used: number; gpu_memory_total: number; }
export interface FetchGameTimeRequest {}
export interface FetchGameTimeResponse { time_of_day: number; }
export interface SoundUpdateEvent { sound_name: string; }

export type JsEvent =
    | { type: JsEventType.None }
    | { type: JsEventType.InitializeRequest; initialize_request_data: InitializeRequest }
    | { type: JsEventType.BrowseFolderRequest; browse_folder_request_data: BrowseFolderRequest }
    | { type: JsEventType.BrowseFolderResponse; browse_folder_response_data: BrowseFolderResponse }
    | { type: JsEventType.EmptyResponse; empty_response_data: EmptyResponse }
    | { type: JsEventType.LoadDataEvent; load_data_event_data: LoadDataEvent }
    | { type: JsEventType.LoadUpdateEvent; load_update_event_data: LoadUpdateEvent }
    | { type: JsEventType.ListMapsRequest; list_maps_request_data: ListMapsRequest }
    | { type: JsEventType.ListMapsResponse; list_maps_response_data: ListMapsResponse }
    | { type: JsEventType.ListMapPoisRequest; list_map_pois_request_data: ListMapPoisRequest }
    | { type: JsEventType.ListMapPoisResponse; list_map_pois_response_data: ListMapPoisResponse }
    | { type: JsEventType.EnterWorldRequest; enter_world_request_data: EnterWorldRequest }
    | { type: JsEventType.LoadingScreenShowEvent; loading_screen_show_event_data: LoadingScreenShowEvent }
    | { type: JsEventType.LoadingScreenProgressEvent; loading_screen_progress_event_data: LoadingScreenProgressEvent }
    | { type: JsEventType.LoadingScreenCompleteEvent; loading_screen_complete_event_data: LoadingScreenCompleteEvent }
    | { type: JsEventType.AreaUpdateEvent; area_update_event_data: AreaUpdateEvent }
    | { type: JsEventType.WorldPositionUpdateEvent; world_position_update_event_data: WorldPositionUpdateEvent }
    | { type: JsEventType.FpsUpdateEvent; fps_update_event_data: FpsUpdateEvent }
    | { type: JsEventType.SystemUpdateEvent; system_update_event_data: SystemUpdateEvent }
    | { type: JsEventType.FetchGameTimeRequest; fetch_game_time_request_data: FetchGameTimeRequest }
    | { type: JsEventType.FetchGameTimeResponse; fetch_game_time_response_data: FetchGameTimeResponse }
    | { type: JsEventType.SoundUpdateEvent; sound_update_event_data: SoundUpdateEvent };