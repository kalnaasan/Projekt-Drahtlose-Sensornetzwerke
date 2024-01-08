import {Injectable} from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs";

@Injectable({
  providedIn: 'root'
})
export class ApiService {

  constructor(private http: HttpClient) {
  }

  public getValueBySensorTypeAndDate(sensorName: string, from: string, to: string): Observable<any> {
    return this.http.get<any>(`http://localhost:8080/api/v1/web/values?sensor-name=${sensorName}&from=${from}&to=${to}`);
  }
  public getSensorsByRoom(roomId: string): Observable<any>{
    return this.http.get<any>(`http://localhost:8080/api/v1/web/sensors?room-id=${roomId}`);
  }
}
