import {InitializeRequest, JsEvent} from "../proto/js_event";
import {Injectable} from "@angular/core";

@Injectable({providedIn: 'root'})
export class EventService {
    private eventMap = new Map<string, (event: JsEvent) => void>();

    initialize() {
        const event: JsEvent = {
            event: {
                oneofKind: "initializeRequest",
                initializeRequest: {}
            }
        };

        window.cefQuery({
            persistent: true,
            request: JSON.stringify(JsEvent.toJson(event)),
            onSuccess: (response) => {
                const ev = JsEvent.fromJson(JSON.parse(response), {});
                const listener = this.eventMap.get(ev.event.oneofKind as string);
                if (listener) {
                    listener(ev);
                } else {
                    console.warn("No listener for event type: ", ev.event.oneofKind);
                }
            },
            onFailure: (error_code, error_message) => {
                console.error("Error from CEF:", error_message);
            }
        });
    }

    listenForEvent(type: string, callback: (event: JsEvent) => void) {
        this.eventMap.set(type, callback);
    }

    sendMessage(event: JsEvent): Promise<void> {
        return new Promise((resolve, reject) => {
            window.cefQuery({
                persistent: false,
                request: JSON.stringify(JsEvent.toJson(event)),
                onSuccess: (response) => {
                    if (JsEvent.fromJson(JSON.parse(response), {}).event.oneofKind !== "emptyResponse") {
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

    browseFolder(title: string, defaultPath: string, filters: string[], allowCreate: boolean): Promise<string> {
        const event: JsEvent = {
            event: {
                oneofKind: "browseFolderRequest",
                browseFolderRequest: {
                    title: title,
                    defaultPath: defaultPath,
                    filters: filters,
                    allowCreate: allowCreate
                }
            }
        };

        return new Promise((resolve, reject) => {
            window.cefQuery({
                persistent: false,
                request: JSON.stringify(JsEvent.toJson(event)),
                onSuccess: (response) => {
                    const ev = JsEvent.fromJson(JSON.parse(response), {}).event;
                    if (ev.oneofKind !== "browseFolderResponse") {
                        reject("Invalid response from CEF");
                        return;
                    }

                    resolve(ev.browseFolderResponse.path);
                },
                onFailure: (error_code, error_message) => {
                    reject(error_message);
                }
            })
        });
    }
}