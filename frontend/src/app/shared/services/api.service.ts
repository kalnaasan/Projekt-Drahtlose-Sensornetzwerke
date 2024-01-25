import {Injectable} from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs";
import {environment} from "../../../environments/environment";

@Injectable({
  providedIn: 'root'
})
export class ApiService {

  private apiUrl = environment.SERVER_URL + '/api/v1/web';

  constructor(private http: HttpClient) {
  }

  public getValueBySensorTypeAndDate(sensorName: string, from: string|undefined, to: string|undefined): Observable<any> {
    return this.http.get<any>(`${this.apiUrl}/values?sensor-name=${sensorName}&from=${from}&to=${to}`);
  }
  public getSensorsByRoom(roomId: string): Observable<any>{
    return this.http.get<any>(`${this.apiUrl}/sensors?room-id=${roomId}`);
  }
}
