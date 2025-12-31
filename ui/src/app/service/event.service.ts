import {JsEvent, JsEventType} from "./js-event";
import {Injectable} from "@angular/core";

@Injectable({providedIn: 'root'})
export class EventService {
    private eventMap = new Map<JsEventType, (event: JsEvent) => void>();

    initialize() {
        const event: JsEvent = {
            type: JsEventType.InitializeRequest,
            initialize_request_data: {}
        };

        window.cefQuery({
            persistent: true,
            request: JSON.stringify(event),
            onSuccess: (response) => {
                const ev = JSON.parse(response) as JsEvent;
                const listener = this.eventMap.get(ev.type);
                if (listener) {
                    listener(ev);
                }
            },
            onFailure: (error_code, error_message) => {
                console.error("Error", error_code, "from CEF:", error_message);
            }
        });
    }

    listenForEvent(type: JsEventType, callback: (event: JsEvent) => void) {
        this.eventMap.set(type, callback);
    }

    sendMessage(event: JsEvent): Promise<void> {
        return new Promise((resolve, reject) => {
            window.cefQuery({
                persistent: false,
                request: JSON.stringify(event),
                onSuccess: (response) => {
                    if (JSON.parse(response).type !== JsEventType.EmptyResponse) {
                        console.warn("Ignoring response from CEF: ", response);
                    }
                    resolve();
                },
                onFailure: (error_code, error_message) => {
                    console.warn("Error sending event ", event, " to CEF: ", error_code, ": ", error_message);
                    reject(error_message);
                }
            })
        })
    }

    sendMessageWithResponse(event: JsEvent): Promise<JsEvent> {
        return new Promise((resolve, reject) => {
            window.cefQuery({
                persistent: false,
                request: JSON.stringify(event),
                onSuccess: (response) => {
                    const resp = JSON.parse(response) as JsEvent;
                    resolve(resp);
                },
                onFailure: (error_code, error_message) => {
                    console.warn("Error sending event ", event, " to CEF: ", error_code, ": ", error_message);
                    reject(error_message);
                }
            })
        })
    }

    browseFolder(title: string, defaultPath: string, filters: string[], allowCreate: boolean): Promise<string> {
        const event: JsEvent = {
            type: JsEventType.BrowseFolderRequest,
            browse_folder_request_data: {
                title: title,
                default_path: defaultPath,
                filters: filters,
                allow_create: allowCreate
            }
        };

        return new Promise((resolve, reject) => {
            window.cefQuery({
                persistent: false,
                request: JSON.stringify(event),
                onSuccess: (response) => {
                    console.log(response);
                    const ev = JSON.parse(response) as JsEvent;
                    if (ev.type !== JsEventType.BrowseFolderResponse) {
                        reject("Invalid response from CEF");
                        return;
                    }

                    resolve(ev.browse_folder_response_data.path);
                },
                onFailure: (error_code, error_message) => {
                    reject(error_code + ": " + error_message);
                }
            })
        });
    }
}