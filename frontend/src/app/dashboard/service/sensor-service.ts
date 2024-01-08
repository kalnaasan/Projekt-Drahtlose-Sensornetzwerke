import {Injectable} from "@angular/core";
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs";
import {Room} from "../common/room";
import {Sensor} from "../common/sensor";

@Injectable({
  providedIn: 'root',
})

export class SensorService {

  private apiUrl = 'http://localhost:8080/api/v1/web/sensors';
  constructor(private http: HttpClient) {}

  getAllSensors(): Observable<Sensor[]> {
    return this.http.get<Sensor[]>(this.apiUrl);
  }


}
