import {InitializeRequest, JsEvent} from "../proto/js_event";
import {Injectable} from "@angular/core";

@Injectable({providedIn: 'root'})
export class EventService {
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

            },
            onFailure: (error_code, error_message) => {
                console.error("Error from CEF:", error_message);
            }
        });
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