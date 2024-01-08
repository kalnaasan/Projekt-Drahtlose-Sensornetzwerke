import {Injectable} from "@angular/core";
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs";
import {Sensor} from "../../dashboard/common/sensor";
import {environment} from "../../../environments/environment";

@Injectable({
  providedIn: 'root',
})

export class SensorService {

  private apiUrl = environment.SERVER_URL + '/api/v1/web/sensors';

  constructor(private http: HttpClient) {
  }

  getAllSensors(): Observable<Sensor[]> {
    return this.http.get<Sensor[]>(this.apiUrl);
  }
}
