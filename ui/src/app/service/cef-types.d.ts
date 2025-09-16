interface CefQueryOptions {
    persistent: boolean;
    request: string;
    onSuccess?: (response: string) => void;
    onFailure?: (error_code: number, error_message: string) => void;
}

interface Window {
    cefQuery(options: CefQueryOptions): void;
}