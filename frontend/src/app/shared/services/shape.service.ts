import {Injectable} from '@angular/core';
import {environment} from "../../../environments/environment";
import {HttpClient} from "@angular/common/http";
import {Observable} from "rxjs";
import {Shape} from "../model/shape";

@Injectable({
  providedIn: 'root'
})
export class ShapeService {
  private apiUrl = environment.SERVER_URL + '/api/v1/web/shapes';

  constructor(private http: HttpClient) {
  }

  public getShapes(): Observable<any> {
    return this.http.get<any>(this.apiUrl);
  }

  public createShape(shape: Shape): Observable<any> {
    return this.http.post<any>(this.apiUrl, shape);
  }

  getShapeById(id: string): Observable<any> {
    return this.http.get<any>(`${this.apiUrl}/${id}`);
  }

  updateShape(shape: Shape) {
    return this.http.put<any>(`${this.apiUrl}/${shape.id}`, shape);

  }
}
